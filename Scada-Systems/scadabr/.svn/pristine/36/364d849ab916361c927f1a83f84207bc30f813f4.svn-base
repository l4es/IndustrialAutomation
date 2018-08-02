package br.org.scadabr.view.component;

import br.org.scadabr.DataType;
import com.serotonin.mango.view.component.ViewComponent;
import com.serotonin.mango.vo.User;
import br.org.scadabr.utils.i18n.LocalizableMessage;
import br.org.scadabr.utils.i18n.LocalizableMessageImpl;
import java.util.Set;

abstract public class CustomComponent extends ViewComponent {

    abstract public String generateContent();

    abstract public String generateInfoContent();

    @Override
    public boolean isCustomComponent() {
        return true;
    }

    public Set<DataType> getSupportedDataTypes() {
        return definition().getSupportedDataTypes();
    }

    public String getTypeName() {
        return definition().getName();
    }

    public LocalizableMessage getDisplayName() {
        return new LocalizableMessageImpl(definition().getNameKey());
    }

    @Override
    public boolean isValid() {
        return true;
    }

    @Override
    public boolean isVisible() {
        // TODO Auto-generated method stub
        return true;
    }

    @Override
    public void validateDataPoint(User user, boolean makeReadOnly) {

    }

}
